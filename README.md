# protoc-gen-improbable-eng-grpc-web

Protoc plugin for genrating @improbable-eng/grpc-web compatible service definitions in TypeScript. The [official plugin](https://github.com/improbable-eng/ts-protoc-gen) only outputs `.d.ts` and `.js` files which became problamatic while trying to incorporate grpc-web in our [bazel](https://bazel.build/) built application. This plugin creates `.ts` files which we feed into a `ts_library` in bazel.

## Install

```python
http_archive(
    name = "minga_protoc_gen_improbable_eng_grpc_web",
    strip_prefix = "protoc-gen-improbable-eng-grpc-web-20c375eb99e5e32524613d5baff91964ada66fc0",
    urls = ["https://github.com/ChangeGamers/protoc-gen-improbable-eng-grpc-web/archive/20c375eb99e5e32524613d5baff91964ada66fc0.zip"],
    sha256 = "c941033a41f73988df4535d72330c5f0a593bb48bb4ca445f8168c427e59e5dd",
)
```

## Contributing

Pull requests are welcome
