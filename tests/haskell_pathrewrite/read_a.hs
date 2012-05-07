-- Cabal will normally hardcode some $prefix for data files etc
-- So if we can show that an attempt to read file a can be intercepted and swapped with reading b,
-- then we could swap the prefix just as well
main = do putStrLn "Will read a.txt ..."
          putStrLn "If intercepted, contents of B should appear"
          contents <- readFile "a.txt"
          putStrLn contents