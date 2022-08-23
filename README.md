# Unicode Library

Collection of high-level wrappers above ICU.

## `unicode::string_view`

View over unicode characters inside any string-like thing.

### Why?
Most programming languages, even with unicode support, fail to recognize flags and denormalized characters as single entities.

python:
```py
```

C++:
```c++
```

Except, swift:
```swift
```

## Features
Main features of `unicode::string_view`:
* Indexing over unicode characters (grapheme clusters), not codepoints
* O(1) time and memory overhead for ASCII strings
* O(1) size() complexity 
* O(log n) operator[] complexity
