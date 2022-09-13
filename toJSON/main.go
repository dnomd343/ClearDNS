package main

import (
    "encoding/json"
    "fmt"
    "github.com/BurntSushi/toml"
    "gopkg.in/yaml.v3"
    "os"
)

func parser(raw []byte) (interface{}, interface{}) {
    var object interface{}
    if err := json.Unmarshal(raw, &object); err == nil { // try json
        return object, nil // json format
    }
    if err := toml.Unmarshal(raw, &object); err == nil { // try toml
        return object, nil // toml format
    }
    if err := yaml.Unmarshal(raw, &object); err == nil { // try yaml
        return object, nil // yaml format
    }
    return nil, nil // parser error
}

func main() {
    if len(os.Args) < 2 { // without argument
        fmt.Printf("usage: toJSON [file]\n")
        os.Exit(0)
    }
    raw, err := os.ReadFile(os.Args[1])
    if err != nil {
        os.Exit(2) // file open failed
    }
    if object, err := parser(raw); err == nil {
        ret, _ := json.Marshal(object)
        fmt.Println(string(ret))
        os.Exit(0)
    }
    os.Exit(1) // unmarshal failed
}
