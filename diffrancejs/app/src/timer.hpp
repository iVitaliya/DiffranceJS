#include <v8.h>
#include <uv.h>

#include "./util.hpp"

struct timer
{
    v8::Global<v8::Function> callback;
    v8::Isolate *isolate;
    uv_timer_t uvTimer;
};

uv_loop_t *loop;

class Timer
{
    public:
        static void Initialise(uv_loop_t *evloop)
        {
            loop = evloop;
        }

        static void SetTimeout(const v8::FunctionCallbackInfo<v8::Value> &args)
        {
            auto isolate = args.GetIsolate();
            auto context = isolate->GetCurrentContext();
            v8::Local<v8::Value> callback = args[0];
            int64_t delay = args[1]->IntegerValue(context).ToChecked();

            if (!callback->IsFunction())
            {
                ThrowTypeError(isolate, "Invalid execTimeout() callback function provided");

                return;
            }

            timer *timerWrap = new timer();

            timerWrap->callback.Reset(isolate, callback.As<v8::Function>());
            timerWrap->isolate = isolate;

            uv_timer_t *timer = new uv_timer_t;
            timer->data = (void *)timerWrap;

            uv_timer_init(loop, timer);
            uv_timer_start(timer, onTimeoutCallback, delay, 0);
        }

        static void onTimeoutCallback(uv_timer_t *handle)
        {
            timer *timerWrap = (timer *)handle->data;

            if (!timerWrap)
            {
                return;
            }

            v8::Isolate *isolate = timerWrap->isolate;

            if (isolate->IsDead())
            {
                // Don't proceed if isolate is dead.
                delete timerWrap;

                return;
            }

            v8::TryCatch tryCatch(isolate);

            v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, timerWrap->callback);
            v8::Local<v8::Value> result;

            if (!callback->Call(isolate->GetCurrentContext(), v8::Undefined(isolate), 0, nullptr).ToLocal(&result))
            {
                ReportException(isolate, &tryCatch);
            }

            delete timerWrap;
        }

        static void SetImmediate(const v8::FunctionCallbackInfo<v8::Value> &args)
        {
            auto isolate = args.GetIsolate();
            auto context = isolate->GetCurrentContext();
            v8::Local<v8::Value> callback = args[0];

            if (!callback->IsFunction())
            {
                ThrowTypeError(isolate, "Invalid execImmediate() callback function provided");

                return;
            }

            timer *timerWrap = new timer();

            timerWrap->callback.Reset(isolate, callback.As<v8::Function>());
            timerWrap->isolate = isolate;

            uv_async_t *async = new uv_async_t;
            async->data = (void *)timerWrap;

            uv_async_init(loop, async, onImmediateCallback);
            uv_async_send(async);
        }

        static void onImmediateCallback(uv_async_t *handle)
        {
            timer *timerWrap = (timer *)handle->data;

            if (!timerWrap)
            {
                return;
            }

            v8::Isolate *isolate = timerWrap->isolate;

            if (isolate->IsDead())
            {
                // Don't proceed if isolate is dead.
                delete timerWrap;

                return;
            }

            v8::TryCatch tryCatch(isolate);
            v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, timerWrap->callback);
            v8::Local<v8::Value> result;

            if (!callback->Call(isolate->GetCurrentContext(), v8::Undefined(isolate), 0, nullptr).ToLocal(&result))
            {
                ReportException(isolate, &tryCatch);
            }

            delete timerWrap;
            uv_close((uv_handle_t *)handle, onDeleteHandle);
        }

        static void onDeleteHandle(uv_handle_t *handle)
        {
            delete (timer *)handle->data;
            delete handle;
        }

        static void SetInterval(const v8::FunctionCallbackInfo<v8::Value> &args)
        {
            auto isolate = args.GetIsolate();
            auto context = isolate->GetCurrentContext();
            v8::Local<v8::Value> callback = args[0];
            int64_t interval = args[1]->IntegerValue(context).ToChecked();

            if (!callback->IsFunction())
            {
                ThrowTypeError(isolate, "Invalid execInterval() callback function provided");

                return;
            }

            timer *timerWrap = new timer();

            timerWrap->callback.Reset(isolate, callback.As<v8::Function>());
            timerWrap->isolate = isolate;

            uv_timer_t *timer = new uv_timer_t;
            timer->data = (void *)timerWrap;

            uv_timer_init(loop, timer);
            uv_timer_start(timer, onIntervalCallback, interval, interval);
        }

        static void onIntervalCallback(uv_timer_t *handle)
        {
            timer *timerWrap = (timer *)handle->data;

            if (!timerWrap)
            {
                return;
            }

            v8::Isolate *isolate = timerWrap->isolate;

            if (isolate->IsDead())
            {
                // Don't proceed if isolate is dead.
                delete timerWrap;

                return;
            }

            v8::TryCatch tryCatch(isolate);
            v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, timerWrap->callback);
            v8::Local<v8::Value> result;

            if (!callback->Call(isolate->GetCurrentContext(), v8::Undefined(isolate), 0, nullptr).ToLocal(&result))
            {
                ReportException(isolate, &tryCatch);
            }
        }
};