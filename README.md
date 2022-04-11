# Simple Makefile Generator - `makeGen`

`makeGen` is a program that generates a simple Makefile for either a small project or as a starting point for a more complex Makefile. I created this simple program to save time when creating Makefiles for small projects. 

The generated Makefile just contains two rules `all` and `clean`. <br>
The rules are created as follows:

```

all:
    $(CC) $(CFLAGS) -o executableName $(TARGETS)
    
clean:
    rm -f executableName
    
```

