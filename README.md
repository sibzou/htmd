# htmd

A project of markdown to HTML converter. It reads the markdown on `stdin` and prints the HTML on `stdout`.

```bash
$ ./build
$ ./htmd < document.md > document.html
```

Or just try it faster:

```bash
$ echo "hello you can click [here](https://kernel.org)" | ./htmd
```

and see the result in the console.

## What is working?

- [x] paragraphs
- [x] links
- [ ] headings
- [ ] code blocks
- [ ] unordered lists
