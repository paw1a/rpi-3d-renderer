#!/usr/bin/env bash
find . \(  -name "*.aux" \
        -o -name "*.bbl" \
        -o -name "*.bcf" \
        -o -name "*.blg" \
        -o -name "*.fdb_latexmk" \
        -o -name "*.fls" \
        -o -name "*.log" \
        -o -name "*.toc" \
        -o -name "*.out" \
        -o -name "*.run.xml" \
        -o -name "*.synctex.gz" \
        -o -name "*.synctex(busy)" \
        -o -name "report.pdf" \) \
        -exec rm {} \;
