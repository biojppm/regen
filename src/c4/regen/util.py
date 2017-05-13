
debug_mode = True


def dbg(*args, **kwargs):
    if debug_mode:
        print(*args, **kwargs)


def cacheattr(obj, name, function):
    """add and cache an object member which is the result of a given function.
    This is for implementing lazy getters when the function call is expensive."""
    if hasattr(obj, name):
        val = getattr(obj, name)
    else:
        val = function()
        setattr(obj, name, val)
    return val

