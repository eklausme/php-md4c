--TEST--
Indented code blocks
--EXTENSIONS--
md4c
--FILE--
<?php
$md = <<<'EOS'
a simple

    indented code block
EOS;
echo md4c_toHtml($md);
echo "---\n";
echo md4c_toHtml($md, MD4C_DIALECT_GITHUB);
?>
--EXPECT--
<p>a simple</p>
<p>indented code block</p>
---
<p>a simple</p>
<pre><code>indented code block
</code></pre>
