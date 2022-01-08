# htmd

A project of markdown to HTML converter. It reads the markdown on `stdin` and prints the HTML on `stdout`.

```bash
$ ./build
$ ./htmd < document.md > document.html
```

Or just try it faster:

```bash
$ echo "my markdown here" | ./htmd
```

and see the result in the console.

## What is working?

- [x] paragraphs
- [ ] links - **work in progress**
- [ ] headings
- [ ] code blocks
- [ ] unordered lists
