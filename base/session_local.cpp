// Copyright 2016 Husky Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "base/session_local.hpp"

#include <functional>
#include <vector>

namespace base {

bool SessionLocal::session_end_ = true;

std::vector<std::function<void()>>& SessionLocal::get_initializers() {
    static std::vector<std::function<void()>> initializers;
    return initializers;
}

std::vector<std::function<void()>>& SessionLocal::get_finalizers() {
    static std::vector<std::function<void()>> finalizers;
    return finalizers;
}

std::vector<std::function<void()>>& SessionLocal::get_thread_finalizers() {
    static thread_local std::vector<std::function<void()>> thread_finalizers;
    return thread_finalizers;
}

void SessionLocal::register_initializer(std::function<void()> init) { get_initializers().push_back(init); }

void SessionLocal::register_finalizer(std::function<void()> fina) { get_finalizers().push_back(fina); }

void SessionLocal::register_thread_finalizer(std::function<void()> fina) { get_thread_finalizers().push_back(fina); }

void SessionLocal::initialize() {
    if (session_end_) {
        for (auto func : get_initializers())
            func();
        session_end_ = false;
    }
}

void SessionLocal::finalize() {
    if (!session_end_) {
        for (auto func : get_finalizers())
            func();
        session_end_ = true;
    }
}

void SessionLocal::thread_finalize() {
    if (!session_end_) {
        for (auto func : get_thread_finalizers())
            func();
    }
}

bool SessionLocal::is_session_end() { return session_end_; }

RegSessionInitializer::RegSessionInitializer(std::function<void()> init) { SessionLocal::register_initializer(init); }

RegSessionFinalizer::RegSessionFinalizer(std::function<void()> fina) { SessionLocal::register_finalizer(fina); }

RegSessionThreadFinalizer::RegSessionThreadFinalizer(std::function<void()> fina) {
    SessionLocal::register_thread_finalizer(fina);
}

}  // namespace base
