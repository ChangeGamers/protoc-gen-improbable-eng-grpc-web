# protoc-gen-improbable-eng-grpc-web

Protoc plugin for genrating @improbable-eng/grpc-web compatible service definitions in TypeScript. The [official plugin](https://github.com/improbable-eng/ts-protoc-gen) only outputs `.d.ts` and `.js` files which became problamatic while trying to incorporate grpc-web in our [bazel](https://bazel.build/) built application. This plugin creates `.ts` files which we feed into a `ts_library` in bazel.

## Install

```python
http_archive(
    name = "minga_protoc_gen_improbable_eng_grpc_web",
    strip_prefix = "protoc-gen-improbable-eng-grpc-web-1e55435c2316955aeb0d92f907e04b7dac269add",
    urls = ["https://github.com/ChangeGamers/protoc-gen-improbable-eng-grpc-web/archive/1e55435c2316955aeb0d92f907e04b7dac269add.zip"],
    sha256 = "63966506ba2f953b916047f10651f6f833025168229cc4f30979ef0c942fabb8",
)
```

## Contributing

Pull requests are welcome
