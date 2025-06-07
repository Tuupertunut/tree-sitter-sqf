package tree_sitter_sqf_test

import (
	"testing"

	tree_sitter "github.com/tree-sitter/go-tree-sitter"
	tree_sitter_sqf "github.com/tuupertunut/tree-sitter-sqf/bindings/go"
)

func TestCanLoadGrammar(t *testing.T) {
	language := tree_sitter.NewLanguage(tree_sitter_sqf.Language())
	if language == nil {
		t.Errorf("Error loading SQF grammar")
	}
}
