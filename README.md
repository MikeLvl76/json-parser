# JSON parser

Just a light parser written in C on Windows :scream: :scream: :scream: (I will port it on Linux).

# How to run it (Windows)

- Install [MinGW](https://sourceforge.net/projects/mingw/) to compile C/C++ programs on Windows.

- Compile

    ```powershell
    $ gcc -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wconversion -Wsign-conversion -Wstringop-overflow "json-parser.c" -o "json-parser"
    ```

- Run

    ```powershell
    $ .\"json-parser" -h
    ```

## CLI arguments

- **Note:** commands order doesn't matter but at least one must be provided

<table>
    <tr class="header">
        <th class="header-cell">Name</th>
        <th class="header-cell">Usage</th>
        <th class="header-cell">Description</th>
    </tr>
    <tr>
        <td>Help</td>
        <td class="usage">
            <code class="code">--help</code>
            <code class="code">-h</code>
        </td>
        <td>
            Display commands usage. 
        </td>
    </tr>
    <tr>
        <td>Default</td>
        <td class="usage">
            <code class="code">--default</code>
            <code class="code">-d</code>
        </td>
        <td>
            Use default JSON files.<br/>
            Instead of parsing all, one is chosen randomly.
        </td>
    </tr>
    <tr>
        <td>Files</td>
        <td class="usage">
            <code class="code">--files &#60;paths&#62;</code>
            <code class="code">-f &#60;paths&#62;</code>
        </td>
        <td>
            Use files by giving their path.<br/>
            Each path must be separated by a whitespace.<br/>
            At least one file path must be provided.
        </td>
    </tr>
    <tr>
        <td>Pretty</td>
        <td class="usage">
            <code class="code">--pretty</code>
            <code class="code">-p</code>
        </td>
        <td>
            Prints JSON content with correct indents and spaces.<br/>
            If not used, prints JSON content like classic string with no indents and spaces.
        </td>
    </tr>
    <tr>
        <td>Show error</td>
        <td class="usage">
            <code class="code">--show-error</code>
            <code class="code">-se</code>
        </td>
        <td>
            Prints parsing errors but does not stop running.<br/>
            If not used, prints nothing even if errors occur.
        </td>
    </tr>
    <tr>
        <td>Stop on error</td>
        <td class="usage">
            <code class="code">--stop-on-error</code>
            <code class="code">-soe</code>
        </td>
        <td>
            Stop running at first encountered error.<br/>
            Useful with <code>--show-error</code> command to see where it failed.<br/>
            If not used, parsing may become uncontrolled.
        </td>
    </tr>
    <tr>
        <td>Indent</td>
        <td class="usage">
            <code class="code">--indent &#60;value&#62;</code>
            <code class="code">-i &#60;value&#62;</code>
        </td>
        <td>
            Set indent (default 0) to modify indent before key/value pairs, arrays or objects.<br/>
            Useful with <code>--prettify</code> command for better print.<br/>
        </td>
    </tr>
</table>