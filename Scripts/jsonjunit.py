import sys
import json
import xml.etree.ElementTree as ET
import pprintpp
import copy

from pathlib import Path

class TestResult:
    name="placeholder"
    classname="placeholder"
    assertions="1"
    time="0"
    file=""
    line=""
    skipReason=""
    failMessage=""
    failType=""
    
    def __str__(self):
        return self.name

class TestSuiteData:
    name=""
    tests=0
    failures=0
    errors=0
    skipped=0
    time=0
    timestamp=""

    def __init__(self,timestamp):
        self.timestamp = timestamp

    def __add__(self, o):
        result = TestSuiteData(self.timestamp)
        result.name = self.name
        result.timestamp = self.timestamp
        result.tests = self.tests + o.tests
        result.failures = self.failures + o.failures
        result.errors = self.errors + o.errors
        result.skipped = self.skipped + o.skipped
        result.time = self.time + o.time
        return result


def storeInTree(treePtr, categoriesList, test):
    if len(categoriesList) == 2:
        if not categoriesList[0] in treePtr:
            treePtr[categoriesList[0]]= []
        treePtr[categoriesList[0]].append(test)
        return
    
    if not categoriesList[0] in treePtr:
        treePtr[categoriesList[0]] = {}
    
    storeInTree(treePtr[categoriesList[0]], categoriesList[1:],test)

def storeListInXML(xmlLocalRoot, testTreeLocalRoot, currentSuiteName, accumulator):
    resultAccumulator = copy.deepcopy(accumulator)

    currentXMLRoot = ET.SubElement(xmlLocalRoot, "testSuite")
    resultAccumulator.name = resultAccumulator.name + "."
    resultAccumulator.name = resultAccumulator.name + currentSuiteName
    currentXMLRoot.set("name", resultAccumulator.name)

    for test in testTreeLocalRoot:
        print(testTreeLocalRoot)
        resultAccumulator.tests = resultAccumulator.tests + 1

        currentTest = ET.SubElement(currentXMLRoot,"testcase")
        currentTest.set("name",test.name)
        currentTest.set("classname",test.classname)
        currentTest.set("assertions",test.assertions)
        currentTest.set("time", str(test.time))

        if test.file:
            currentTest.set("file",test.file)
        if test.line:
            currentTest.set("line", str(test.line))

        if test.failType:
            resultAccumulator.failures = resultAccumulator.failures + 1
            if test.failType == "Error":
                resultAccumulator.errors = resultAccumulator.errors + 1
            failureElement = ET.SubElement(currentTest,"failure")
            failureElement.set("message", test.failMessage)
            failureElement.set("type", test.failType)

        resultAccumulator.time = resultAccumulator.time + test.time

    currentXMLRoot.set("tests",str(resultAccumulator.tests))
    currentXMLRoot.set("failures",str(resultAccumulator.failures))
    currentXMLRoot.set("errors",str(resultAccumulator.errors))
    currentXMLRoot.set("skipped",str(resultAccumulator.skipped))
    currentXMLRoot.set("time",str(resultAccumulator.time))

    return resultAccumulator

def storeDictInXML(xmlLocalRoot, testTreeLocalRoot, currentSuiteName, accumulator, indent = 0, isRoot=False):

    originalAccumulator = copy.deepcopy(accumulator)
    resultAccumulator = copy.deepcopy(accumulator)

    # if local root is a dictionary keep digging and propagate results upwards
    if isinstance(testTreeLocalRoot, dict):
        currentXMLRoot=ET.SubElement(xmlLocalRoot, "testsuite")

        if isRoot:
            originalAccumulator.name = currentSuiteName
        else:
            originalAccumulator.name = originalAccumulator.name + "."
            originalAccumulator.name = originalAccumulator.name + currentSuiteName
    
        currentXMLRoot.set("name", originalAccumulator.name)

        for nextSuiteName in testTreeLocalRoot:
            if isinstance(testTreeLocalRoot[nextSuiteName], dict):
                localAccumulator = storeDictInXML(currentXMLRoot, testTreeLocalRoot[nextSuiteName],nextSuiteName,originalAccumulator, indent=indent+1)
            else:
                localAccumulator = storeListInXML(xmlLocalRoot, testTreeLocalRoot[nextSuiteName], nextSuiteName, originalAccumulator)
            resultAccumulator = resultAccumulator + localAccumulator
        
        currentXMLRoot.set("tests",str(resultAccumulator.tests))
        currentXMLRoot.set("failures",str(resultAccumulator.failures))
        currentXMLRoot.set("errors",str(resultAccumulator.errors))
        currentXMLRoot.set("skipped",str(resultAccumulator.skipped))
        currentXMLRoot.set("time",str(resultAccumulator.time))

        return resultAccumulator

def main():
    """
    This is the main function where the program's primary logic resides.
    """
    
    file = Path(sys.argv[1])

    if not file.is_file():
        raise  ValueError("The path [" + sys.argv[1] + "] is not a file")

    with open(file,mode='r',encoding='utf-8-sig') as jsonFile:
        data = json.load(jsonFile)

    # Create the root element
    root = ET.Element("testsuites", name="All Tests", tests=str(data["succeeded"]+data["succeededWithWarnings"]+data["failed"]+data["notRun"]), failures=str(data["failed"]),skipped=str(data["notRun"]), time=str(data["totalDuration"]), timestamp=str(data["reportCreatedOn"]))

    testTree = {}

    for test in data["tests"]:
        categories = test["fullTestPath"].split('.')
        testResult = TestResult()
        testResult.name = categories[-1]
        testResult.classname = ".".join(categories[:-1])
        testResult.time = test["duration"]
        if test["state"] == "Fail":
            testResult.file = test["entries"][0]["filename"].replace("\\","/")
            testResult.line = test["entries"][0]["lineNumber"]
            testResult.failMessage = test["entries"][0]["event"]["message"] 
            testResult.failType = test["entries"][0]["event"]["type"] 
        storeInTree(testTree,categories,testResult)

    originalAccumulator = TestSuiteData(data["reportCreatedOn"])
    accumulator = TestSuiteData(data["reportCreatedOn"])

    for testSuite in testTree:
        localAccumulator =  storeDictInXML(root, testTree[testSuite], testSuite, originalAccumulator , isRoot=True)
        accumulator = accumulator + localAccumulator
    
    root.set("tests",str(accumulator.tests))
    root.set("failures",str(accumulator.failures))
    root.set("errors",str(accumulator.errors))
    root.set("skipped",str(accumulator.skipped))
    root.set("time",str(accumulator.time))


    # Create an ElementTree object
    tree = ET.ElementTree(root)
    ET.indent(tree, space=" ", level=0)
    # Write the XML to a file
    try:
        tree.write(sys.argv[2], encoding="utf-8", xml_declaration=True)
        print("XML file created successfully.")
    except Exception as e:
        print(f"Error writing XML file: {e}")
    
if __name__ == "__main__":
    main()