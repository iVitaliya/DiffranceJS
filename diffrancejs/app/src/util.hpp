#include "v8.h"

static inline v8::Local<v8::String> v8_str(const char *x)
{
    return v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), x).ToLocalChecked();
}

// Copied from https://github.com/v8/v8/blob/master/samples/shell.cc
const char *ToCString(const v8::String::Utf8Value &value)
{
    return *value ? *value : "<string conversion failed>";
}

// Copied from ChatGPT
void ReportException(v8::Isolate *isolate, v8::TryCatch *try_catch)
{
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Message> message = try_catch->Message();

    if (!message.IsEmpty())
    {
        v8::String::Utf8Value exception(isolate, try_catch->Exception());
        v8::String::Utf8Value filename(isolate, message->GetScriptOrigin().ResourceName());
        int linenum = message->GetLineNumber(isolate->GetCurrentContext()).FromJust();
        
        fprintf(stderr, "An Error has occurred in \"%s\" at line: %d\nException: %s", *filename, linenum, *exception);

        v8::String::Utf8Value sourceline(isolate, message->GetSourceLine(isolate->GetCurrentContext()).ToLocalChecked());
        fprintf(stderr, "\n\n%s\n", *sourceline);

        // Print wavy underline
        int start = message->GetStartColumn(isolate->GetCurrentContext()).FromJust();

        for (int i = 0; i < start; i++)
        {
            fprintf(stderr, " ");
        }

        int end = message->GetEndColumn(isolate->GetCurrentContext()).FromJust();

        for (int i = 0; i < end; i++)
        {
            fprintf(stderr, "^");
        }

        fprintf(stderr, "\n");

        v8::Local<v8::Value> stack_trace_string;
        
        if (try_catch->StackTrace(isolate->GetCurrentContext()).ToLocal(&stack_trace_string) &&
            stack_trace_string->IsString() &&
            v8::Local<v8::String>::Cast(stack_trace_string)->Length() > 0)
        {
            v8::String::Utf8Value stack_trace(isolate, stack_trace_string);
            
            fprintf(stderr, "%s\n", *stack_trace);
        }
    }
}

void ThrowTypeError(v8::Isolate *isolate, const char *message)
{
    v8::Local<v8::String> errorMessage = v8::String::NewFromUtf8(isolate, message, v8::NewStringType::kNormal).ToLocalChecked();

    isolate->ThrowException(v8::Exception::TypeError(errorMessage));
}