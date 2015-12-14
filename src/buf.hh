namespace node_libtidy {

  class Buf {

  public:

    Buf() { tidyBufInitWithAllocator(&buf, &allocator); }

    ~Buf() {
      tidyBufFree(&buf);
    }

    operator TidyBuffer*() {
      return &buf;
    }

    void empty() {
      buf.next = buf.size = 0;
    }

    v8::MaybeLocal<v8::String> string() const {
      return Nan::New<v8::String>(data(), buf.size);
    }

    v8::MaybeLocal<v8::Object> buffer() const {
      return Nan::CopyBuffer(data(), buf.size);
    }

  private:

    TidyBuffer buf;

    char* data() const {
      return reinterpret_cast<char*>(buf.bp);
    }

  };
  
}