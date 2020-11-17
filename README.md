# cin-conic
simple webframwork writing in c language

- Support simple Restful API router
    + GET ,POST
    + URL param
- File transfer in response body
    + HTML, PNG, JPG
    + support static file in project directory

## Run
```
$ make
$ sudo ./main
```

## Result
### Example main
```c
void param(Context *ctx) {
    char *arg;
    if (GetData(ctx->req.map, "arg", (void**)&arg, cpyString) >= 0) {
        SendMsg(ctx, StatusOK, arg);
        free(arg);
    } else {
        SendMsg(ctx, StatusInternalServerError, "param error");
    }
}

int main(int argc, char *argv[]) {
    Router *r = NewRouter();
    Static(r, "view");
    Static(r, "view/img");

    GET(r, "/", helloWorld);
    GET(r, "/image/png", png);
    GET(r, "/param/:arg", param);

    StartServer(r, "8080");

    return 0;
}
```

![image](https://user-images.githubusercontent.com/44857109/97798807-ee994580-1c6c-11eb-9567-d04bbe25f54a.png)
