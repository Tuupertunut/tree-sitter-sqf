import XCTest
import SwiftTreeSitter
import TreeSitterSqf

final class TreeSitterSqfTests: XCTestCase {
    func testCanLoadGrammar() throws {
        let parser = Parser()
        let language = Language(language: tree_sitter_sqf())
        XCTAssertNoThrow(try parser.setLanguage(language),
                         "Error loading SQF grammar")
    }
}
