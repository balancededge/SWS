# TODO
 1. Test large files
 2. Allow upper/lower case for method, http

```

let $ = match.matchObject;
let _ = match.star;

match(object, {
    $() : $ => (),
});

match(x, [
    1, $ => console.log('one'),
    2, $ => console.log('two'),
    3, $ => console.log('three'),
    _, $ => console.log('anything'),
]);

let c = 'c';

match(c, [
    $({ _ : 'x' }), $ => console.log($.x)
);

match(x, {
    $(1) || $(2), $ => console.log("one or two"),
    3,            $ => console.log("three"),
    _,            $ => console.log("anything"),
}

// An alternative syntax

match(x, ($, _) => [
    $([1, 2]), $ => console.log("one or two"),
    3,         $ => console.log("three"),
    _,         $ => console.log("anything"),
]);

match(origin, [
    $(Point, { x : 'x', y : 'y' }), $ => console.log($.x + ',' + $.y),
]);

match(origin, [
    $(Point, { x : 'x1', y : 'y1' }), $ => console.log($.x1 + ',' + $.y1),
]);

match(x, [
    $.range(1, 5), $ => console.log("one through five"),
    _,             $ => console.log("anything")
]);

match(x, [
    $.range('a', 'j'), $ => console.log('early letter'),
    $.range('k', 'z'), $ => console.log('late letter'),
    _,                 $ => console.log('something else')
]);

match(x, [
    $.if(y, $([4, 5])) => console.log('no'),
    _,                 => console.log('no')
])

match x {
    4 | 5 if y => println!("yes"),
    _ => println!("no"),
}



```