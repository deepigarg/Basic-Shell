
# Input Convention:
Run shell.c\
Enter the command without its path if “/bin/command” results in an error. The
program detects the path on its own.
# Types of commands handled:
Few examples of types of commands executed:

1. Basic commands:
- ls -l
- grep -i C file.txt
- man
- cd ..
- mkdir

2. Input redirection:
- grep c<filename
- sort<file.txt

3. Output redirection:
- cat a.txt>file
- ls>file
- ls 1>file
- 2>&1
- 2>file
- grep a a.txt>file

4. Output Appending:
- ls>>filename

5. Pipes:
- cat f.txt | grep a
- ls | sort | uniq | wc -l
- cat f.txt | sort | uniq

6. exit
