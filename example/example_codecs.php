<?php
var_dump(bsdconv_module_check(BSDCONV_FROM,"_utf-8"));
var_dump(bsdconv_module_check(BSDCONV_INTER,"_utf-8"));
echo "Filter:\n";
print_r(bsdconv_modules_list(BSDCONV_FILTER));
echo "From:\n";
print_r(bsdconv_modules_list(BSDCONV_FROM));
echo "Inter:\n";
print_r(bsdconv_modules_list(BSDCONV_INTER));
echo "To:\n";
print_r(bsdconv_modules_list(BSDCONV_TO));
?>
