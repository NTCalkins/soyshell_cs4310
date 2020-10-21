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
<h2>Set-up</h2>
<p>
  Navigate to the root of the directory and run <code>make</code> to build everything. The main executable will be named "soyshell". Run it with <code>./soyshell</code>.
</p>
<h2>Grammar</h2>
<p>
  The syntax of expressions processed by soyshell follows this grammar.<br><br>
  <strong>
    ('+' = mandatory presence of whitespace)<br>
    expr: s | s + op + expr<br>
    s: {expr} | invoke<br>
    invoke: cmd [+ '|' + cmd]...<br>
    op: && | '||' | ; | =<br>
    redir: &lt; | &gt; | &gt;&gt;<br>
    cmd: EXECUTABLE [+ arg]... [+ redir + FILE_NAME]... [+ &]<br>
    arg: $NAMED_CONSTANT | LITERAL<br>
  </strong><br>
  This mimics the syntax of most POSIX shells with the exception of the = operator.
</p>
<h2>Behavioral Nuances</h2>
<p>
  For the purposes of the assignment, the default value for PATH consists solely of the current directory the shell was called in plus "/bin". This ensures that the shell will use the commands written for the assignment by default. Additional directories can be appended to the path variable using the = operator with ':' as the separator (e.g. PATH = $PATH:/bin).<br>
  Due to the nature of the grammar and our decision to treat the assignment operator = as a proper operator and not an exception of cmd, = must be separated by white space. This differs from the behavior of most shells. Additionally, in the case of cascading output redirection (e.g. ls > foo.txt > bar.txt), output will only be written to the file associated with the last redirection operator (bar.txt in this case). This matches the behavior of some shells but noticeably differs from the behavior of shells like zsh.
</p>
