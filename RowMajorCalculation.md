While working on a recent gaming project, I was originally using 2D arrays to store information relating to the different levels in the game. But when it came to loop through the contents of these levels, it wasn't as straightforward to do a simple foreach loop due to the multiple dimensions.

Instead, I changed the code so that the 2D data was stored in a single dimension array. By using row-major order you can calculate any position in 2D space and map it into the 1D array. This then allows you to continue accessing the data using 2D co-ordinates, but opens up 1D access too.
Defining your array

Given the size of your 2D array, the 1D creation code is trivial:

T[] items = new T[width * height];

Converting 2D co-ordinates into 1D index

Once your have your array, converting a 2D co-ordinate such as 3, 4 into the correct index of your 1D array using row-major order using the following formula:

y * width + x

Converting 1D index into 2D co-ordinates

The calculation to convert a 1D index into a 2D co-ordinate is fairly straightforward:

y = index / width;
x = index % width;

Putting it together - the ArrayMap<T> class

To avoid constantly having to repeat the calculations, I created a generic ArrayMap class that I could use to store any data type in a 1D array, and access the values using either indexes or co-ordinates, as well as adding enumerable support. The class is very straightforward to use:

ArrayMap<int> grid;
Size size;
int value;

size = new Size(10, 10);
value = 0;
grid = new ArrayMap<int>(size);

// set values via 2D co-ordinates
for (int y = 0; y < size.Height; y++)
{
  for (int x = 0; x < size.Width; x++)
  {
    grid[x, y] = value;
    value++;
  }
}

// get values via 2D co-ordinates
Console.WriteLine(grid[9, 0]); // 9
Console.WriteLine(grid[0, 9]); // 90
Console.WriteLine(grid[9, 9]); // 99

// set values via indexes
for (int i = 0; i < grid.Count; i++)
  grid[i] = i;

// get values via index
Console.WriteLine(grid[9]); // 9
Console.WriteLine(grid[90]); // 90
Console.WriteLine(grid[99]); // 99

// enumerate items
foreach (int i in grid)
  Console.WriteLine(i);

// get index
Console.WriteLine(grid.GetItemIndex(9, 9)); // 99

// get location
Console.WriteLine(grid.GetItemLocation(99)); // 9,9

Below is the full source to the class.

using System;
using System.Collections;
using System.Collections.Generic;

namespace BinaryRealms.Engine
{
  public class ArrayMap<T> : IEnumerable<T>
  {
    private T[] _items;
    private Size _size;

    public ArrayMap()
    { }

    public ArrayMap(int width, int height)
      : this(new Size(width, height))
    { }

    public ArrayMap(Size size)
      : this()
    {
      this.Size = size;
    }

    public IEnumerator<T> GetEnumerator()
    {
      foreach (T item in _items)
        yield return item;
    }

    public int GetItemIndex(int x, int y)
    {
      if (x < 0 || x >= this.Size.Width)
        throw new IndexOutOfRangeException("X is out of range");
      else if (y < 0 || y >= this.Size.Height)
        throw new IndexOutOfRangeException("Y is out of range");

      return y * this.Size.Width + x;
    }

    public int GetItemIndex(Point point)
    {
      return this.GetItemIndex(point.X, point.Y);
    }

    public Point GetItemLocation(int index)
    {
      Point point;

      if (index < 0 || index >= _items.Length)
        throw new IndexOutOfRangeException("Index is out of range");

      point = new Point();
      point.Y = index / this.Size.Width;
      point.X = index % this.Size.Width;

      return point;
    }

    public int Count
    { get { return _items.Length; } }

    public Size Size
    {
      get { return _size; }
      set
      {
        _size = value;
        _items = new T[_size.Width * _size.Height];
      }
    }

    public T this[Point location]
    {
      get { return this[location.X, location.Y]; }
      set { this[location.X, location.Y] = value; }
    }

    public T this[int x, int y]
    {
      get { return this[this.GetItemIndex(x, y)]; }
      set { this[this.GetItemIndex(x, y)] = value; }
    }

    public T this[int index]
    {
      get { return _items[index]; }
      set { _items[index] = value; }
    }

    IEnumerator IEnumerable.GetEnumerator()
    {
      return this.GetEnumerator();
    }
  }
}

Currently I'm using this class without any problems, but if you spot any errors or think it could do with anything else, please let me know!
