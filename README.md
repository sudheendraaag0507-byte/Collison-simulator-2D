# GOT MY HANDS DIRTY WHILE CREATING A 2D COLLISION SIMULATOR

## A Short Journal On My Realisations and Journey Into The Depth Of Cpp...

First, when I started to write my very first code for developing this simulator, 
it was just console inputs. It was pretty smooth, and next came the OpenGL object rendering and matrix manipulation—the floor had been set.

And as days passed by, I was like, "How do I even render a circle in OpenGL? All I can do is draw solid polygons.
Then, after searching dozens of websites, I found the nested loop technique, 
which was totally inefficient and had a terrifying syntax that had a mental tax, lol. Okay, fine. 
Then I asked my close buddy Gemini; he took me into the world of shaders and I discovered this beautiful keyword **"discard"**. 
This will remove the unwanted fragment, and I learned that GLSL is nothing but modified C. 
After realizing the truth, I was unable to control myself and I started playing with GLSL, did some cool stuff, and wasted one whole day on it.

Then I tried to create my very first circle and again started to play with it.
Literally, I created the Olympic logo in it, and yeah, it was fun though! And I got to learn that,
in the graphics pipeline, the data travels as individual points at the vertex shader and converts into fragments with individual coordinates, 
using interpolation math.

Then I rendered a ball... **IT WASN'T A BALL, IT WAS AN EGG!** Since when I stretched my window, 
the NDC coordinates were transformed, and I was like, "Huh... please don't do this."
Then an idea popped; I just made use of the interpolation math I learned. When we pass the coordinates down the vertex shader, 
they get interpolated into fragments. So, what I did was I sent the unmodified coordinates down, and modified the coordinates (changed their ratios) and passed them to `gl_Position`. 
This way, the rasterizer would place the square with its ratio modified according to the NDC coordinate system,
and the fragments get their coordinates as per the unmodified coordinates we passed down.

Finally got the shape right, and now it's time to create multiple balls and make them collide. 
From here, the journey and the iterations were smooth, and BAM! The sneaky coordinate manipulation backfired.
When the balls hit, it satisfied the condition (i.e., distance between them = sum of their radii), but as we changed the ratio, 
from the CPU point of view, the particle was hit, but from the GPU point of view, the collision didn't happen. 
Because of the ratio change we did, the ball was completely squished according to the NDC.

So I was thinking, and arrived at an idea: why don't I completely dive into shaders? If the particles are overlapping,
then their fragments should also overlap. Then I started searching on how to do it, and I realized that it is far too advanced to comprehend how the system works.
Plus, driving the data all the way from VRAM to RAM was completely slow, so I was put into a complete dilemma of whether to look into compute shading or not.
My brain was split into two: one was saying, "This seems interesting, just dig a little deeper and it's worth learning," and the other was like, 
"It's not worth your time; if you start exploring, you will be distracted completely." So, I was stuck in the same thought for two whole days.

Then I had a massive realization that anything could be solved using math, and after all, these algorithms are just a mutation of math and physics.
Thankfully, I was learning OOP at that time, and I was like, "I need to make use of what I learned..."

Then at the middle of the night, I got an idea. Wait, **"objects in programming are objects in real life.

"** Why don't I treat particles like one, and why don't I create a grid virtually? Basically,
I tried to mimic what a fragment shader was doing on the CPU; I tried to create my own virtual pixel window on RAM. 

And TADA!! The virtual grid was born! I tried to mimic what rasterization did and implement the exact idea here in a much more efficient manner. 
After that, I implemented the required math and tried to paint the virtual window on RAM using the particle positions and checked for overlaps.
To create this window, I didn't create a 2D array but rather a **static 1D array** because it is faster to cache.

So yeah, then you might wonder why I have used a list here—let's come to it later. 
Then, after doing all the required math, I was introduced to the `static` keyword. 
I had been wondering about the same thing since my first semester and it was quite unclear for me, but then it became clear. 
These are the keywords that help in allocating memory for the same in a code segment (a region in RAM). 
As it is static, it can't allocate memory when declared as it does in the case of a normal variable, because this would cause a problem. 
If a static variable is given memory the moment it is declared, then that static variable will lose its purpose. 
That is, when multiple declarations happen across files, the compiler tends to allocate memory each time. 
This is more likely in the case of classes, as they will be reused endlessly. 
So, in order to prevent that, the compiler was designed to allocate memory only if it is initialized, so after that, naturally,
initializing it again and again doesn't change the memory.

That was because I was overwhelmed with all these pointers by myself, so I just wanted some break from it.
And yeah, this is where I realized how compilers are beautifully designed, and I can't wait to study about compiler design.

Nope, not yet! And yeah, thus, I tried to mimic the rasterizer and check for the overlapping of particles using a list, maintained a history of it,
and avoided invoking the math multiple times. And yeah, I thought the journey was complete. YAYYY!!!

But the real demon appeared when I tried to simulate 500 particles:
the quantum tunneling or bullet effect in software terms (fun fact: I actually discovered that quantum tunneling exists in real life!).
It literally ate my sleep. 

For three whole nights, I was debugging a lot thanks to that bug. Because of that,
I was able to discover three completely unrelated bugs which were killing the performance of the engine, and yeah, the dot product math was born this way. 
It prevented the balls from binding into one another and sticking into the wall. The math is simple:
if the dot product between the relative position and velocity is positive, then the ball is moving away (no collision), and when negative, it is moving closer.
So thus, I just avoided nested conditional statements with one line of math.

And finally, I found a solution for the bug which caused the ghost particle effect today. I realized that the particles were teleporting. 
That was entirely because my velocity math was absurd from the start—that is, I assumed each frame time increments by 1 second, and that was my mistake. 
So, I did time stepping by assuming 0.1 seconds passes for each frame. This way, 
I was able to avoid almost every ghost particle which I was seeing before, and now I am able to simulate 5,000 particles of various sizes without breaking a sweat!

And yeah, I am onto further optimization, like completely removing the `for` loop for drawing images which is acting as a bottleneck while rendering more particles. 

Updates are upcoming! This is my very first article. Hope you enjoyed this journey!

---

### 💻 Clone the Repository
```bash
[git clone [https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git](https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git)](https://github.com/sudheendraaag0507-byte/Collison-simulator-2D.git)
