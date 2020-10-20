<h1>CS4310 Soyshell</h1>
<p>
  Soyshell is a simple shell written in C as an exercise in creating and managing processes using the standard POSIX library. The shell current supports the following features.
  <ul>
    <li>Running executable files both by specifying the absolute path as well as by specifying only the filename to be searched for in all the directories listed in PATH</li>
    <li>Running processes in the background with &amp</li>
    <li>Input/output redirection using &lt;, &gt;, and &gt;&gt;</li>
    <li>Piping using |</li>
    <li>Conditional execution using &amp;&amp; and ||</li>
    <li>Defining constants using = (NOTE: Unlike most shells, = must be separated by spaces (e.g. PATH = $PATH:/bin)</li>
    <li>Expansion of constants in argument lists using $</li>
  </ul>
</p>
<h3>Grammar</h3>
<p>
  The syntax of expressions processed by soyshell follows this grammar.<br>
  <code>
    ('+' = mandatory presence of whitespace)
    expr: s | s + op + expr
    s: {expr} | invoke
    invoke: cmd [+ '|' + cmd]...
    op: && | '||' | ; | =
    redir: &lt; | &gt; | &gt;&gt;
    cmd: EXECUTABLE [+ arg]... [+ redir + FILE_NAME]... [+ &]
    arg: $NAMED_CONSTANT | LITERAL
  </code>
</p>
<h3>Behavioral Nuances</h3>
<p>
  For the purposes of the assignment, the default value for PATH consists solely of the current directory the shell was called in plus "/bin". This ensures that the shell will use the commands written for the assignment by default. Additional directories can be appended to the path variable using the = operator with ':' as the separator (e.g. PATH = $PATH:/bin).
  Due to the nature of the grammar and our decision to treat the assignment operator = as a proper operator and not an exception of cmd, = must be separated by white space. This differs from the behavior of most shells. Additionally, in the case of cascading output redirection (e.g. ls > foo.txt > bar.txt), output will only be written to the file associated with the last redirection operator (bar.txt in this case). This matches the behavior of some shells but noticeably differs from the behavior of shells like zsh.
</p>
