using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MagneticPositioningGUI.Extensions
{
    public static class CMatrixExtension
    {
        public static double[] GetRowElements(this CMatrix.Matrix matrix, int row)
        {
            if (row > matrix.rows || row < 0)
                throw new ArgumentException("row number is not in range", nameof(row));
            var elements = new List<double>();
            for(int i = 0; i < matrix.cols ;++i)
            {
                elements.Add(matrix.Data[row, i]);
            }
            return elements.ToArray();
        }

        public static double[] GetColumnElements(this CMatrix.Matrix matrix, int column)
        {
            if (column > matrix.cols || column < 0)
                throw new ArgumentException("column number is not in range", nameof(column));
            var elements = new List<double>();
            for (int i = 0; i < matrix.rows; ++i)
            {
                elements.Add(matrix.Data[i, column]);
            }
            return elements.ToArray();
        }

    }
}
