
debug_mode = True


def dbg(*args, **kwargs):
    if debug_mode:
        print(*args, **kwargs)
