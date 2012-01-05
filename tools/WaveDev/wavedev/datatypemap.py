import os

datatypes = {}
datatypes["boolean"] = "Boolean"
datatypes["char"] = "Char"
datatypes["double"] = "Double"
datatypes["float"] = "Float"
datatypes["short"] = "Short"
datatypes["long"] = "Long"
datatypes["objref"] = "Object_var"
datatypes["octet"] = "Octet"
datatypes["string"] = "String_var"
datatypes["ulong"] = "ULong"
datatypes["ushort"] = "UShort"

def getDatatype(thetype):
    try:
        str_type = datatypes[thetype]
    except KeyError:
        str_type = ""
    return str_type

