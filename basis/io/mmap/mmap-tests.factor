USING: alien.c-types alien.data compiler.tree.debugger
io.encodings.ascii io.files io.files.temp io.files.unique
io.mmap kernel locals math sequences sequences.private
specialized-arrays tools.test ;
IN: io.mmap.tests

SPECIALIZED-ARRAY: uint

[
    "mmap-test-file" ".txt" [| path |
        "12345" path ascii set-file-contents
        { } [ path [ char <mapped-array> CHAR: 2 0 pick set-nth drop ] with-mapped-file ] unit-test
        { 5 } [ path [ char <mapped-array> length ] with-mapped-file ] unit-test
        { 5 } [ path [ char <mapped-array> length ] with-mapped-file-reader ] unit-test
        { "22345" } [ path ascii file-contents ] unit-test
        { t } [ path uint [ sum ] with-mapped-array integer? ] unit-test
        { t } [ path uint [ sum ] with-mapped-array-reader integer? ] unit-test
    ] cleanup-unique-file

    "mmap-empty-file" ".txt" [| path |
        [ path [ drop ] with-mapped-file ] [ bad-mmap-size? ] must-fail-with
    ] cleanup-unique-file
] with-temp-directory

{ t } [
    [ "test.txt" <mapped-file> void* <c-direct-array> first-unsafe ]
    { nth-unsafe } inlined?
] unit-test
