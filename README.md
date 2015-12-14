# libtidy

This package provides bindings to
[libtidy](http://www.html-tidy.org/developer/) a.k.a.
[TidyLib](http://api.html-tidy.org/tidy/tidylib_api_5.1.25/tidylib.html)
which can be used to parse and tidy up HTML 5.
The library is built as a native node extension,
so you don't have to have the HTML Tidy package installed on your system.

## Alternatives

* [tidy-html5](https://www.npmjs.com/package/tidy-html5)
  has libtidy compiles to JavaScript using emscripten.
  It is likely more portable, but at the cost of performance.
* [tidy](https://www.npmjs.com/package/tidy)
  and [tidy2](https://www.npmjs.com/package/tidy2)
  also provide bindings for libtidy,
  but they expect the library and its header files
  to be installed on the system.
* [htmltidy](https://www.npmjs.com/package/htmltidy)
  and [htmltidy2](https://www.npmjs.com/package/htmltidy2)
  use the command line tool to tidy up html,
  so the tool has to be installed and
  they incur some process creation overhead.