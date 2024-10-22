--TEST--
Basic conversion
--EXTENSIONS--
md4c
--FILE--
<?php
$md = <<<'EOS'
Title
=====

Some text.
EOS;
echo md4c_toHtml($md);
?>
--EXPECT--
<h1>Title</h1>
<p>Some text.</p>
