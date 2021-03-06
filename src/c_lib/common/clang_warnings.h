#pragma once

#ifdef __clang__

# pragma clang diagnostic warning "-Wabi"
# pragma clang diagnostic warning "-Waddress-of-temporary"
# pragma clang diagnostic warning "-Waddress"
# pragma clang diagnostic warning "-Waggregate-return"
# pragma clang diagnostic warning "-Wall"
# pragma clang diagnostic warning "-Wambiguous-member-template"
# pragma clang diagnostic warning "-Warc-abi"
# pragma clang diagnostic warning "-Warc-non-pod-memaccess"
# pragma clang diagnostic warning "-Warc-retain-cycles"
# pragma clang diagnostic warning "-Warc-unsafe-retained-assign"
# pragma clang diagnostic warning "-Warc"
# pragma clang diagnostic warning "-Watomic-properties"
# pragma clang diagnostic warning "-Wattributes"
# pragma clang diagnostic warning "-Wavailability"
# pragma clang diagnostic warning "-Wbad-function-cast"
# pragma clang diagnostic warning "-Wbind-to-temporary-copy"
# pragma clang diagnostic warning "-Wbitwise-op-parentheses"
# pragma clang diagnostic warning "-Wbool-conversions"
# pragma clang diagnostic warning "-Wbuiltin-macro-redefined"
# pragma clang diagnostic warning "-Wc++-compat"
# pragma clang diagnostic warning "-Wc++0x-compat"
//# pragma clang diagnostic warning "-Wc++0x-extensions"
# pragma clang diagnostic warning "-Wcast-align"
# pragma clang diagnostic warning "-Wcast-qual"
# pragma clang diagnostic warning "-Wchar-align"
# pragma clang diagnostic warning "-Wchar-subscripts"
# pragma clang diagnostic warning "-Wcomment"
# pragma clang diagnostic warning "-Wcomments"
# pragma clang diagnostic warning "-Wconditional-uninitialized"
# pragma clang diagnostic warning "-Wconversion"
# pragma clang diagnostic warning "-Wctor-dtor-privacy"
# pragma clang diagnostic warning "-Wcustom-atomic-properties"
# pragma clang diagnostic warning "-Wdeclaration-after-statement"
# pragma clang diagnostic warning "-Wdelegating-ctor-cycles"
# pragma clang diagnostic warning "-Wdelete-non-virtual-dtor"
# pragma clang diagnostic warning "-Wdeprecated-declarations"
# pragma clang diagnostic warning "-Wdeprecated-implementations"
# pragma clang diagnostic warning "-Wdeprecated-writable-strings"
# pragma clang diagnostic warning "-Wdeprecated"
# pragma clang diagnostic warning "-Wdisabled-optimization"
# pragma clang diagnostic warning "-Wdiscard-qual"
# pragma clang diagnostic warning "-Wdiv-by-zero"
# pragma clang diagnostic warning "-Wduplicate-method-arg"
# pragma clang diagnostic warning "-Weffc++"
# pragma clang diagnostic warning "-Wempty-body"
# pragma clang diagnostic warning "-Wendif-labels"
# pragma clang diagnostic warning "-Wenum-compare"
//# pragma clang diagnostic warning "-Wexit-time-destructors"
# pragma clang diagnostic warning "-Wextra-tokens"
# pragma clang diagnostic warning "-Wextra"
# pragma clang diagnostic warning "-Wformat-extra-args"
//# pragma clang diagnostic warning "-Wformat-nonliteral"
# pragma clang diagnostic warning "-Wformat-zero-length"
# pragma clang diagnostic warning "-Wformat"
# pragma clang diagnostic warning "-Wformat=2"
# pragma clang diagnostic warning "-Wfour-char-constants"
//# pragma clang diagnostic warning "-Wglobal-constructors"
# pragma clang diagnostic warning "-Wgnu-designator"
//# pragma clang diagnostic warning "-Wgnu"
# pragma clang diagnostic warning "-Wheader-hygiene"
# pragma clang diagnostic warning "-Widiomatic-parentheses"
# pragma clang diagnostic warning "-Wignored-qualifiers"
# pragma clang diagnostic warning "-Wimplicit-atomic-properties"
# pragma clang diagnostic warning "-Wimplicit-function-declaration"
# pragma clang diagnostic warning "-Wimplicit-int"
# pragma clang diagnostic warning "-Wimplicit"
# pragma clang diagnostic warning "-Wimport"
# pragma clang diagnostic warning "-Wincompatible-pointer-types"
# pragma clang diagnostic warning "-Winit-self"
# pragma clang diagnostic warning "-Winitializer-overrides"
//# pragma clang diagnostic warning "-Winline"
# pragma clang diagnostic warning "-Wint-to-pointer-cast"
# pragma clang diagnostic warning "-Winvalid-offsetof"
# pragma clang diagnostic warning "-Winvalid-pch"
# pragma clang diagnostic warning "-Wlarge-by-value-copy"
# pragma clang diagnostic warning "-Wliteral-range"
# pragma clang diagnostic warning "-Wlocal-type-template-args"
# pragma clang diagnostic warning "-Wlogical-op-parentheses"
# pragma clang diagnostic warning "-Wlong-long"
# pragma clang diagnostic warning "-Wmain"
# pragma clang diagnostic warning "-Wmicrosoft"
# pragma clang diagnostic warning "-Wmismatched-tags"
# pragma clang diagnostic warning "-Wmissing-braces"
# pragma clang diagnostic warning "-Wmissing-declarations"
# pragma clang diagnostic warning "-Wmissing-field-initializers"
# pragma clang diagnostic warning "-Wmissing-format-attribute"
# pragma clang diagnostic warning "-Wmissing-include-dirs"
//# pragma clang diagnostic warning "-Wmissing-noreturn"
# pragma clang diagnostic warning "-Wmost"
# pragma clang diagnostic warning "-Wmultichar"
# pragma clang diagnostic warning "-Wnested-externs"
# pragma clang diagnostic warning "-Wnewline-eof"
# pragma clang diagnostic warning "-Wnon-gcc"
# pragma clang diagnostic warning "-Wnon-virtual-dtor"
# pragma clang diagnostic warning "-Wnonnull"
# pragma clang diagnostic warning "-Wnonportable-cfstrings"
# pragma clang diagnostic warning "-Wnull-dereference"
# pragma clang diagnostic warning "-Wold-style-cast"
# pragma clang diagnostic warning "-Wold-style-definition"
# pragma clang diagnostic warning "-Wout-of-line-declaration"
# pragma clang diagnostic warning "-Woverflow"
# pragma clang diagnostic warning "-Woverlength-strings"
# pragma clang diagnostic warning "-Woverloaded-virtual"
//# pragma clang diagnostic warning "-Wpacked"
//# pragma clang diagnostic warning "-Wpadded"
# pragma clang diagnostic warning "-Wparentheses"
# pragma clang diagnostic warning "-Wpointer-arith"
# pragma clang diagnostic warning "-Wpointer-to-int-cast"
# pragma clang diagnostic warning "-Wprotocol"
# pragma clang diagnostic warning "-Wreadonly-setter-attrs"
# pragma clang diagnostic warning "-Wredundant-decls"
# pragma clang diagnostic warning "-Wreorder"
# pragma clang diagnostic error "-Wreturn-type"
# pragma clang diagnostic warning "-Wself-assign"
# pragma clang diagnostic warning "-Wsemicolon-before-method-body"
# pragma clang diagnostic warning "-Wsequence-point"
//# pragma clang diagnostic warning "-Wshadow"
# pragma clang diagnostic warning "-Wshorten-64-to-32"
# pragma clang diagnostic warning "-Wsign-compare"
# pragma clang diagnostic warning "-Wsign-promo"
# pragma clang diagnostic warning "-Wsizeof-array-argument"
# pragma clang diagnostic warning "-Wstack-protector"
# pragma clang diagnostic warning "-Wstrict-aliasing"
# pragma clang diagnostic warning "-Wstrict-overflow"
# pragma clang diagnostic warning "-Wstrict-prototypes"
# pragma clang diagnostic warning "-Wstrict-selector-match"
# pragma clang diagnostic warning "-Wsuper-class-method-mismatch"
# pragma clang diagnostic warning "-Wswitch-default"
# pragma clang diagnostic warning "-Wswitch-enum"
# pragma clang diagnostic warning "-Wswitch"
# pragma clang diagnostic warning "-Wsynth"
# pragma clang diagnostic warning "-Wtautological-compare"
# pragma clang diagnostic warning "-Wtrigraphs"
# pragma clang diagnostic warning "-Wtype-limits"
# pragma clang diagnostic warning "-Wundeclared-selector"
# pragma clang diagnostic warning "-Wuninitialized"
//# pragma clang diagnostic warning "-Wunknown-pragmas"
# pragma clang diagnostic warning "-Wunnamed-type-template-args"
# pragma clang diagnostic warning "-Wunneeded-internal-declaration"
# pragma clang diagnostic warning "-Wunneeded-member-function"
# pragma clang diagnostic warning "-Wunused-argument"
# pragma clang diagnostic warning "-Wunused-exception-parameter"
//# pragma clang diagnostic warning "-Wunused-function"
# pragma clang diagnostic warning "-Wunused-label"
# pragma clang diagnostic warning "-Wunused-member-function"
//# pragma clang diagnostic warning "-Wunused-parameter"
# pragma clang diagnostic warning "-Wunused-value"
# pragma clang diagnostic warning "-Wunused-variable"
# pragma clang diagnostic warning "-Wunused"
# pragma clang diagnostic warning "-Wused-but-marked-unused"
//# pragma clang diagnostic warning "-Wvariadic-macros"
# pragma clang diagnostic warning "-Wvector-conversions"
# pragma clang diagnostic warning "-Wvla"
# pragma clang diagnostic warning "-Wvolatile-register-var"
# pragma clang diagnostic warning "-Wwrite-strings"

# ifndef __APPLE__
// Not recognized by Apple implementation:
#  pragma clang diagnostic warning "-Wc++0x-narrowing"
//#  pragma clang diagnostic warning "-Wc++11-compat"
//#  pragma clang diagnostic warning "-Wc++11-extensions"
//#  pragma clang diagnostic warning "-Wc++11-extra-semi"
#  pragma clang diagnostic warning "-Wc++11-narrowing"
//#  pragma clang diagnostic warning "-Wc++98-compat-pedantic"
#  pragma clang diagnostic warning "-Wmalformed-warning-check"
#  pragma clang diagnostic warning "-Wmissing-method-return-type"
#  pragma clang diagnostic warning "-Woverriding-method-mismatch"
#  pragma clang diagnostic warning "-Wsentinel"
#  pragma clang diagnostic warning "-Wunused-comparison"
#  pragma clang diagnostic warning "-Wunused-result"
#  pragma clang diagnostic warning "-Wuser-defined-literals"
#  if __clang_major__ > 3
#   pragma clang diagnostic warning "-Wc++11-extra-semi"
#   pragma clang diagnostic ignored "-Wunused-private-field"
#   pragma clang diagnostic ignored "-Wtautological-constant-out-of-range-compare"
#  endif
# endif

// Turn these off
# pragma clang diagnostic ignored "-Wc++11-compat"
# pragma clang diagnostic ignored "-Wunused-function"
# pragma clang diagnostic ignored "-Wconversion"
# pragma clang diagnostic ignored "-Wunused-parameter"
# pragma clang diagnostic ignored "-Wformat-nonliteral"
# pragma clang diagnostic ignored "-Wc++11-extensions" // only ignoring this because it complains about commas at end of enum lists and there's not a more specific warning flag
# if PRODUCTION
#  pragma clang diagnostic ignored "-Winline"
# endif

#endif
