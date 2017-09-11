This is a remake of the borfenstein engine, which will (hopefully) use significantly faster algorithms for rendering, while allowing for
easy texture mapping in the future. The idea is that I'll replace the vector math (and particularily, the vector library eigen) with some
faster trig math, and then precompute all the trig values and put them into a table.

The whole project has been a huge pain in my arse so far to be honest. I'm not sure if this is a viable replacement or not, although it really
does run a lot faster than the other engine. I feel like optimisations to the other engine might reduce the difference in preformance though,
particularily if I start using faster square roots. Texture mapping is still going to be a problem with borfenstein 1.0 though.