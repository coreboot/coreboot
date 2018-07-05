# coreboot documentation guidelines

> Documentation is like sex: when it is good, it is very, very good;
> and when it is bad, it is better than nothing.

That said please always try to write documentation! One problem in the
firmware development is the missing documentation. In this document
you will get a brief introduction how to write, submit and publish
documenation to coreboot.

## Preparations

coreboot uses [Sphinx] documentation tool. We prefer the markdown format
over reStructuredText so only embedded ReST is supported. Checkout the
[Markdown Guide] for more information.

### Install Sphinx

Please follow this official [guide].

### Optional

Install [shpinx-autobuild] for rebuilding markdown/rst sources on the fly!

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

## Markdown and Tables

Under Sphinx markdown tables are not supported. Therefore you can use following
code block to write tables in reStructuredText and embed them into the markdown:

    ```eval_rst
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

[coreboot]: https://coreboot.org
[Documentation]: https://review.coreboot.org/cgit/coreboot.git/tree/Documentation
[shpinx-autobuild]: https://github.com/GaretJax/sphinx-autobuild
[guide]: http://www.sphinx-doc.org/en/stable/install.html
[Sphinx]: http://www.sphinx-doc.org/en/master/
[Markdown Guide]: https://www.markdownguide.org/
[Gerrit Guidelines]: https://doc.coreboot.org/gerrit_guidelines.html
[review.coreboot.org]: https://review.coreboot.org
