#!/bin/sh
racket gen-book-table.rkt books-programming > ../../_includes/books-programming.tab
racket gen-book-table.rkt books-security > ../../_includes/books-security.tab
racket gen-book-table.rkt books-other > ../../_includes/books-other.tab
