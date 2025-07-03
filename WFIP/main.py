import json

datafile = open("data.txt", "w")
books = [{"Author":"Wasserman", "Title":"All Of Statistics", "Year Of Publishing":2004}, 
         {"Author":"Martin Henz", "Title":"Structure and Interpretation of Computer Programs, JS Edition", "Year Of Publishing":2022},
         {"Author":"Ronald Dahl", "Title":"Big Friendly Giant", "Year Of Publishing":1982}]
for book in books:
    datafile.write("{\n")
    for key in book:
        datafile.write(f"{key}: {book[key]} \n")
    datafile.write('},\n')

json_filename = "books_data.json"
with open(json_filename, "w") as jsonfile:
    json.dump(books, jsonfile, indent=4)
print("SUCCESS!")