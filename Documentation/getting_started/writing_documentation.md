# coreboot documentation guidelines

> Documentation is like sex: when it is good, it is very, very good;
> and when it is bad, it is better than nothing.

That said please always try to write documentation! One problem in the
firmware development is the missing documentation. In this document
you will get a brief introduction how to write, submit and publish
documentation to coreboot.

## Preparations

coreboot uses [Sphinx] documentation tool. We prefer the markdown format
over reStructuredText so only embedded ReST is supported. Checkout the
[Markdown Guide] for more information.

### option 1: Use the docker image

The easiest way to build the documentation is using a docker image.
To build the image run the following in the base directory:

	make -C util/docker/ doc.coreboot.org

Before building the documentation make sure the output directory is given
the correct permissions before running docker.

	mkdir -p Documentation/_build

To build the documentation:

	make -C util/docker docker-build-docs

To have the documentation build and served over a web server live run:

	make -C util/docker docker-livehtml-docs

On the host machine, open a browser to the address <http://0.0.0.0:8000>.

### option 2: Install Sphinx

Please follow this official [guide] to install sphinx.
You will also need python-recommonmark for sphinx to be able to handle
markdown documentation.

Since some Linux distributions don't package every needed sphinx extension,
the installation via pip in a venv is recommended. You'll need these python3
modules:

* sphinx
* recommonmark
* sphinx_rtd_theme
* sphinxcontrib-ditaa

The following combination of versions has been tested: sphinx 2.3.1,
recommonmark 0.6.0, sphinx_rtd_theme 0.4.3 and sphinxcontrib-ditaa 0.7.

Now change into the `Documentation` folder in the coreboot directory and run
this command in there

	make sphinx

If no error occurs, you can find the generated HTML documentation in
`Documentation/_build` now.

### Optional

Install [sphinx-autobuild] for rebuilding markdown/rst sources on the fly!

## Basic and simple rules

The following rules should be followed in order to get it at least reviewed
on [review.coreboot.org].

Documentation:

1.  Must be written in **markdown** with **embedded reStructuredText**
    format.
2.  Must be written in **English**.
3.  Must be placed into **Documentation/** directory subfolder.
4.  Should follow the same directory structure as **src/** when practical.
5.  Must be referenced from within other markdown files
6.  The commit must follow the [Gerrit Guidelines].
7.  Must have all **lowercase filenames**.
8.  Running text should have a visible width of about **72 chars**.
9.  Should not **duplicate** documentation, but reference it instead.
10.  Must not include the same picture in multiple markdown files.
11.  Images should be kept small. They should be under 700px in width, as
     the current theme doesn't allow bigger images.
12.  Shouldn't cover implementation details; for details, the code is the
     reference.

## Referencing markdown documents

Starting with Sphinx 1.6 recommonmark's *auto_doc_ref* feature is broken.
To reference documents use the TOC tree or inline RST code.

## Markdown and Tables

Under Sphinx markdown tables are not supported. Therefore you can use following
code block to write tables in reStructuredText and embed them into the markdown:

    ```{eval-rst}
    +------------+------------+-----------+
    | Header 1   | Header 2   | Header 3  |
    +============+============+===========+
    | body row 1 | column 2   | column 3  |
    +------------+------------+-----------+
    | body row 2 | Cells may span columns.|
    +------------+------------+-----------+
    | body row 3 | Cells may  | - Cells   |
    +------------+ span rows. | - contain |
    | body row 4 |            | - blocks. |
    +------------+------------+-----------+
    ``` #just a code block is enough

## TocTree

To make sure that all documents are included into the final documentation, you
must reference each document from at least one *toctree*. The *toctree* must
only reference files in the same folder or in subfolders !
To create a toctree, simply use a bullet list or numbered list with a single
reference. References in regular text aren't considered as *toctree* .
This feature is enabled by recommonmark's *enable_auto_toc_tree* .

**Example toctree:**

```
* [Chapter 1](chapter1.md)
* [Chapter 2](chapter2.md)
* [Subchapter](sub/index.md)
```

```
1. [Chapter 1](chapter1.md)
2. [Chapter 2](chapter2.md)
```

If you do only reference the document, but do not include it in any toctree,
you'll see the following warning:
**WARNING: document isn't included in any toctree**

## CSV

You can import CSV files and let sphinx automatically convert them to human
readable tables, using the following reStructuredText snipped:

    ```{eval-rst}
    .. csv-table::
       :header: "Key", "Value"
       :file: keyvalues.csv
    ```

Of course this can only be done from a markdown file that is included in the
TOC tree.

[coreboot]: https://coreboot.org
[Documentation]: https://review.coreboot.org/cgit/coreboot.git/tree/Documentation
[sphinx-autobuild]: https://github.com/GaretJax/sphinx-autobuild
[guide]: http://www.sphinx-doc.org/en/stable/install.html
[Sphinx]: http://www.sphinx-doc.org/en/master/
[Markdown Guide]: https://www.markdownguide.org/
[Gerrit Guidelines]: ../contributing/gerrit_guidelines.md
[review.coreboot.org]: https://review.coreboot.org
