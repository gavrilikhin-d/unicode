# String
Swift-inspired string library for C++.

String consist of user-percieved characters (grapheme clusters, in unicode terms) like an ‘Ä’,
which may be represented with multiple Unicode code points including a base character and combining marks.

// TODO: example with wrong size of string in other programming languages, except swift

## Features
Main features of String:
* O(1) overhead for ASCII strings
* O(1) size() average complexity 
* O(1) operator[] average complexity
* minimal memory overhead
* UTF-8, UTF-16, or UTF-32 encoding independence
* lazily calculations and allocations

**This is only what library is targetting now. Some of those may change in future**

## Design
Main idea is to have a reasonable estimation of location for first byte of character.
We than obtain corrected location and character size through meta information

# Algorithm
1. Find number of characters in string in O(n)
2. If `byte size == number of characters` -> this is ASCII string, stop here

3. Determine average byte size as `bytes / number of characters` rounded to nearest integer
4. Store nothing(!) for all contignuous sequences of characters with average byte size.
   This is really important, as it will highly reduce memory usage.
5. For contignuous sequences of characters with different byte size, store:
	* first character index
	* sequence length (will be short, as they rarely occur)
	* character byte size (maybe difference from average size?)
	* difference from estimation for first byte ??? 
	 (if we don't store it, then we must go through all Sequences. In worst case it's O(n))

# Efficiency
There are 3 different kinds of texts we need to be aware of:
1. Mostly ASCII (english, german, etc.)
2. Non-ASCII alphabet + frequent ASCII punctuators (russian, ukrainian, etc.)
3. Non-ASCII alphabet + rare ASCII punctuators (chinese, japanese, etc.)

I expect that String will have good results with 1st and 3rd kinds of text,
but will have a lot of Sequences for 2nd kind of texts.

// TODO: add some benchmarks and memory usage measures for this kinds of texts