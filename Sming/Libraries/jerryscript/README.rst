JerryScript
===========

.. highlight:: bash

A JavaScript Virtual Machine based on JerryScript.

Status
------

Currently we use version v2.2 from JerryScript

Update
------

In order to update the version of JerryScript two tasks have to be executed:

- in this repository to update the submodule version of JerryScript to point to the new version
- in https://github.com/attachix/jerryscript repository that same version has to be merged too.

Once that is done we need to build completely from scratch a new docker container.

Dockerfile
----------

Up to now we have good results with creating a pure JavaScript code using the latest sdk.
There is a docker container that creates the needed environment.
It can be built and run with the following commands::

    docker build -f components/jerryscript/Dockerfile -t jerryscript-ems .
    docker run -it jerryscript-ems

Inside the container a new jerryscript snapshot compiler can be built by calling::

    node /jerryscript/build/bin/jsc.js -o /tmp/test.js.snap /tmp/test.js

Where ``test.js.snap`` is the output file and ``/tmp/test.js`` is an existing JavaScript file.

To update the already precompiled jsc.js in ``web/dev`` you can do the following::

    docker run -v $(pwd)/web/:/web -it jerryscript-ems

And run the following commands inside the container::

    cp -r /jerryscript/build/bin/jsc.* /web/dev

That will require also precompilation of the main.js file.
Inside the container this can be done with the following command::

    node /jerryscript/build/bin/jsc.js -o /web/build/main.js.snap /web/dev/main.js


Configuration variables
-----------------------

.. envvar:: APP_JS_SOURCE_DIR

    default: undefined

    Snap files can be created during the build stage by setting this variable in your project's component.mk file::

        APP_JS_SOURCE_DIR := files

    All ``.js`` files will be compiled into ``.snap`` files and written to :envvar:`APP_JS_SNAPDIR`.


.. envvar:: APP_JS_SNAP_DIR

    default: ``out/jerryscript``

    Location to write generated .snap files.
