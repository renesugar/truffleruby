/*
 * Copyright (c) 2013, 2017 Oracle and/or its affiliates. All rights reserved. This
 * code is released under a tri EPL/GPL/LGPL license. You can use it,
 * redistribute it and/or modify it under the terms of the:
 *
 * Eclipse Public License version 1.0, or
 * GNU General Public License version 2, or
 * GNU Lesser General Public License version 2.1.
 */
package org.truffleruby.language;

import com.oracle.truffle.api.CompilerDirectives.CompilationFinal;
import com.oracle.truffle.api.CompilerDirectives.TruffleBoundary;
import com.oracle.truffle.api.object.DynamicObject;

public class LexicalScope {

    public static final LexicalScope NONE = null;

    private final LexicalScope parent;
    @CompilationFinal private volatile DynamicObject liveModule;

    public LexicalScope(LexicalScope parent, DynamicObject liveModule) {
        assert liveModule == null || RubyGuards.isRubyModule(liveModule);
        this.parent = parent;
        this.liveModule = liveModule;
    }

    public LexicalScope(LexicalScope parent) {
        this(parent, null);
    }

    public LexicalScope getParent() {
        return parent;
    }

    public DynamicObject getLiveModule() {
        return liveModule;
    }

    public void unsafeSetLiveModule(DynamicObject liveModule) {
        this.liveModule = liveModule;
    }

    @TruffleBoundary
    public static DynamicObject resolveTargetModuleForClassVariables(LexicalScope lexicalScope) {
        LexicalScope scope = lexicalScope;

        // MRI logic: ignore lexical scopes (cref) referring to singleton classes
        while (RubyGuards.isSingletonClass(scope.liveModule)) {
            scope = scope.parent;
        }

        return scope.liveModule;
    }

    @Override
    public String toString() {
        return " :: " + liveModule + (parent == null ? "" : parent.toString());
    }


}
