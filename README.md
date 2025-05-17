what is in the library:
### Variable: `is_callable_v<Callable, Args...>`
- Will return true if `Callable(Args...)` can be called.
- Examples of use:
     1. `is_callable_v<void(int), int>` = `true`
     2. `is_callable_v<void(int)>` = `false` (`Callable` expects arguments)
---

### Variable: `is_nothrow_callable_v<Callable, Args...>`
- Will return true if `Callable(Args...)` can be called and `Callable` is marked as a `noexcept` keyword.
- Examples of use:
  
    1. `is_nothrow_callable_v<void(int) noexcept, int>` = `true`
    2. `is_nothrow_callable_v<void(int), int>` = `false` (non `noexcept`)
---
