
import module namespace j = "http://www.jsoniq.org/functions";

count(j:flatten(
  [
    [1, "two", 3],
    [["four", 5], "six"],
    [7, ["eight", 9]]
  ]
))