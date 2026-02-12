
# Known Issues:
- Some of the documentation is out-of-date, and still points to cereal
- Polymorphic serialization of duplicate base subobjects that are non-virtual and independent when serializing with the
duplicate type will correctly load but will incorrectly upcast/point to the first occurrence regardless which object was pointed to during saving.
  ```
     A        B         When object is saved via pointer to Base B,
     |        |         during loading the pointer gets incorrectly
  +--+--------+-----+   linked to Base A, even tho the whole object
  |  |        |     |   was correctly serialized / deserialized
  | Base     Base   |
  |   \       /     |
  |    Derived      | <- Correctly loaded object
  +-----------------+
  ```

