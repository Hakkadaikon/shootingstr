# 🌟 shootingstr 🌟
## Summary  
C based nostr relay.

## Getting Started  
You can run the example with the following command.
* meson and ninja must be installed to run make setup/make build.

```bash
# host
make setup
make build
build/shootingstr

# docker
docker compose up -d
```

## Features

- [ ] NIP-01: Basic protocol flow description
  - [x] Validating an EVENT (Tags not included)
  - [x] Saving an EVENT (Tags not included)
  - [ ] Validating an EVENT (Tags)
  - [ ] Saving an EVENT (Tags)
  - [x] Returning an OK message
  - [ ] REQ Response
    - [ ] ids
    - [ ] authors
    - [ ] kinds
    - [ ] \#\<single-letter\>
    - [ ] since
    - [ ] until
    - [ ] limit

## Dependency  

[libwebsockets](https://github.com/warmcat/libwebsockets) - C based websocket client  
[yyjson](https://github.com/ibireme/yyjson) - C based high performance JSON library  
[meson](https://github.com/mesonbuild/meson) - build automation software.  
[ninja](https://github.com/ninja-build/ninja) - small build system with a focus on speed.  

## Author  
Hakkadaikon

## License  
MIT
