package main

import (
    "encoding/json"
    "fmt"
    "github.com/BurntSushi/toml"
    "gopkg.in/yaml.v3"
)

var jsonTest = `
{
    "field1": "test",
    "field2": {
        "field3": "value",
        "field4": [
            42,
            1024
        ]
    }
}
`

var yamlTest = `
field1: test
field2:
  field3: value
  field4: [42, 1024]
`

var tomlTest = `
field1 = "test"

[field2]
field3 = "value"
field4 = [ 42, 1_024 ]
`

func parser(raw string) (interface{}, interface{}) {
    var object interface{}
    if err := json.Unmarshal([]byte(raw), &object); err == nil { // try json
        return object, nil // json format
    }
    if err := toml.Unmarshal([]byte(raw), &object); err == nil { // try toml
        return object, nil // toml format
    }
    if err := yaml.Unmarshal([]byte(raw), &object); err == nil { // try yaml
        return object, nil // yaml format
    }
    return nil, nil // parser error
}

func main() {
    //if object, err := parser(jsonTest); err == nil {
    //if object, err := parser(yamlTest); err == nil {
    if object, err := parser(tomlTest); err == nil {
        b, _ := json.Marshal(object)
        fmt.Println(string(b))
    }
}
