'use strict';

var replacements = [
  { match: /NanAsyncWorker/g, replace: "Nan::AsyncWorker" },
  { match: /NanAsyncQueueWorker/g, replace: "Nan::AsyncQueueWorker" },
  { match: /NanCallback/g, replace: "Nan::Callback" },
  { match: /NanNewBufferHandle\(([^;]+);/g, replace: "Nan::NewBuffer($1.ToLocalChecked();" },
  { match: /(NanNew(<(v8::)?String>)?\(\"[^\"]*\"\))/g, replace: "$1.ToLocalChecked()" },
  { match: /(NanNew<(v8::)?String>\([^\"][^\;]*);/g, replace: "$1.ToLocalChecked();" },
  { match: /NanNew/g, replace: "Nan::New" },
  { match: /NODE_SET_PROTOTYPE_METHOD/g, replace: "Nan::SetPrototypeMethod" },
  { match: /NODE_SET_METHOD/g, replace: "Nan::SetMethod" },
  { match: /_NAN_METHOD_ARGS_TYPE/g, replace: "Nan::NAN_METHOD_ARGS_TYPE" },
  { match: /(\W)?args(\W)/g, replace: "$1info$2" },
  { match: /(^|\s)(v8::)?Persistent/g, replace: "$1Nan::Persistent" },
  { match: /NanAssignPersistent(<\w+>)?\(([^,]+),\s*([^)]+)\)/g, replace: "$2.Reset($3)" },
  { match: /NanDisposePersistent\(([^\)]+)\)/g, replace: "$1.Reset()" },
  { match: /NanReturnValue/g, replace: "info.GetReturnValue().Set" },
  { match: /NanReturnNull\(\)/g, replace: "info.GetReturnValue().Set(Nan::Null())" },
  { match: /NanScope\(\)/g, replace: "Nan::HandleScope scope" },
  { match: /NanEscapableScope\(\)/g, replace: "Nan::EscapableHandleScope scope" },
  { match: /NanEscapeScope/g, replace: "scope.Escape" },
  { match: /NanReturnUndefined\(\);/g, replace: "return;" },
  { match: /NanUtf8String/g, replace: "Nan::Utf8String" },
  { match: /NanObjectWrapHandle\(([^\)]+)\)/g, replace: "$1->handle()" },
  { match: /(node::)?ObjectWrap/g, replace: "Nan::ObjectWrap" },
  { match: /NanMakeCallback/g, replace: "Nan::MakeCallback" },
  { match: /NanNull/g, replace: "Nan::Null" },
  { match: /NanUndefined/g, replace: "Nan::Undefined" },
  { match: /NanFalse/g, replace: "Nan::False" },
  { match: /NanTrue/g, replace: "Nan::True" },
  { match: /NanThrow(\w+)?Error/g, replace: "Nan::Throw$1Error" },
  { match: /NanError/g, replace: "Nan::Error" },
  { match: /NanGetCurrentContext/g, replace: "Nan::GetCurrentContext" },
  { match: /([a-zA-Z0-9_]+)->SetAccessor\(/g, replace: "Nan::SetAccessor($1, " },
  { match: /NanAdjustExternalMemory/g, replace: "Nan::AdjustExternalMemory" },
  { match: /NanSetTemplate/g, replace: "Nan::SetTemplate" },
  { match: /NanHasInstance\(([^,]+),\s*([^)]+)\)/g, replace: "Nan::New($1)->HasInstance($2)" },
  { match: /NanSetInternalFieldPointer/g, replace: "Nan::SetInternalFieldPointer" }
];

var fs = require('fs');
var path = require('path');
var filepath, contents;

function updateFiles(listing, dir) {
  for(var i in listing) {
    filepath = path.join(dir, listing[i]);
    if(fs.statSync(filepath).isDirectory()) {
      console.log('READING ' + filepath);
      updateFiles(fs.readdirSync(filepath), filepath);
    }
    else if(/\.(h||cc)$/.test(filepath)) {
      console.log('REPLACING ' + filepath);
      contents = fs.readFileSync(filepath).toString();
      for(var j in replacements) {
        contents = contents.replace(replacements[j].match, replacements[j].replace);
      }
      fs.writeFileSync(filepath, contents);
    }
  }
}

updateFiles(fs.readdirSync('./'), __dirname);