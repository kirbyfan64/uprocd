# ruby.module -- The uprocd Ruby module

## SYNOPSIS

ruby.module

## DESCRIPTION

This is the Ruby native module for uprocd.

## PROPERTIES

**Preload=<string>**

    Code to run during the initialization phase.

**Run=<string>**

    Code to run when the module is forked.

## EXAMPLE

```ini
[DerivedModule]
Base=ruby
# This code is to initialize the module.
Preload=require 'time'
# This will be called when the module is run.
Run=
  puts "Running code..."
```
