#pragma once

struct Demo {
    virtual ~Demo() = default;
    virtual const char* getName() const = 0;
    virtual void init() = 0;
    virtual void render() = 0;
    virtual void handleInput() {}
    virtual void destroy() = 0;
};
