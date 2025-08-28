#!/bin/bash

# Test LaTeX compilation script for DB25 tokenizer documentation

echo "Testing LaTeX compilation..."

# Test tutorial-diagrams.tex
echo "Compiling tutorial-diagrams.tex..."
cd docs
pdflatex -interaction=nonstopmode tutorial-diagrams.tex > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "✓ tutorial-diagrams.tex compiled successfully"
else
    echo "✗ tutorial-diagrams.tex failed to compile"
    echo "Running again to show errors:"
    pdflatex -interaction=nonstopmode tutorial-diagrams.tex
fi

# Clean up auxiliary files
rm -f *.aux *.log *.out *.toc

cd ..

# Test academic paper
echo "Compiling db25-tokenizer-paper.tex..."
cd papers
pdflatex -interaction=nonstopmode db25-tokenizer-paper.tex > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "✓ db25-tokenizer-paper.tex compiled successfully"
else
    echo "✗ db25-tokenizer-paper.tex failed to compile"
    echo "Running again to show errors:"
    pdflatex -interaction=nonstopmode db25-tokenizer-paper.tex
fi

# Clean up auxiliary files
rm -f *.aux *.log *.out *.toc *.bbl *.blg

cd ..

echo "LaTeX test complete!"