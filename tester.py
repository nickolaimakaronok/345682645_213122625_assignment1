import subprocess
import random
import math
import os
import sys

# --- קבועים והגדרות ---
C_EXEC = "./kmeans"
PY_SCRIPT = "kmeans.py"
INPUT_FILE = "input_test_data.txt"
EPSILON = 0.001

# צבעים לטרמינל
GREEN = '\033[92m'
RED = '\033[91m'
YELLOW = '\033[93m'
RESET = '\033[0m'

def generate_input(filename, N, dim):
    """מייצר קלט אקראי"""
    points = []
    with open(filename, 'w') as f:
        for _ in range(N):
            # מספרים בטווח רחב כדי לגרום לתזוזה
            vec = [round(random.uniform(-20, 20), 4) for _ in range(dim)]
            points.append(vec)
            f.write(",".join(map(str, vec)) + "\n")
    return points

def dist(v1, v2):
    return math.sqrt(sum((x - y) ** 2 for x, y in zip(v1, v2)))

def run_internal_logic(points, K, max_iter):
    """
    מימוש פנימי 'נכון' של האלגוריתם להשוואה.
    כולל את לוגיקת 'קלאסטר ריק -> העתקת וקטור ראשון'.
    """
    if K >= len(points): return None # שגיאה צפויה
    
    centroids = [p[:] for p in points[:K]]
    
    for _ in range(max_iter):
        clusters = [[] for _ in range(K)]
        
        # שיוך
        for p in points:
            distances = [dist(p, c) for c in centroids]
            min_d = min(distances)
            closest = distances.index(min_d)
            clusters[closest].append(p)
            
        new_centroids = []
        changed = False
        
        # עדכון
        for i in range(K):
            cluster = clusters[i]
            if cluster:
                # חישוב ממוצע
                dim = len(cluster[0])
                avg = [0.0] * dim
                for p in cluster:
                    for d in range(dim):
                        avg[d] += p[d]
                new_c = [x / len(cluster) for x in avg]
                new_centroids.append(new_c)
            else:
                # לוגיקה לקלאסטר ריק: העתקת הוקטור הראשון מהקלט המקורי
                new_centroids.append(points[0][:])
        
        # התכנסות
        for i in range(K):
            if dist(centroids[i], new_centroids[i]) >= EPSILON:
                changed = True
        
        centroids = new_centroids[:]
        if not changed:
            break
            
    return centroids

def run_program(cmd_args, input_file):
    """מריץ תוכנית חיצונית (C או Python) ומחזיר פלט"""
    try:
        with open(input_file, 'r') as infile:
            res = subprocess.run(
                cmd_args, 
                stdin=infile, 
                capture_output=True, 
                text=True, 
                timeout=5 # טיימאאוט למניעת לולאות אינסופיות
            )
        return res.stdout.strip(), res.stderr, res.returncode
    except subprocess.TimeoutExpired:
        return None, "Timeout", 1
    except Exception as e:
        return None, str(e), 1

def parse_centroids(output):
    """ממיר פלט טקסטואלי לרשימת וקטורים"""
    if not output: return []
    res = []
    for line in output.split('\n'):
        try:
            vec = [float(x) for x in line.split(',')]
            res.append(vec)
        except:
            pass
    return res

def compare_vectors(vecs1, vecs2, tolerance=0.0005):
    """משווה שתי רשימות של וקטורים"""
    if len(vecs1) != len(vecs2): return False
    for v1, v2 in zip(vecs1, vecs2):
        if len(v1) != len(v2): return False
        if dist(v1, v2) > tolerance: return False
    return True

def run_test(test_name, K, iter_val, N, dim, expect_error=None):
    print(f"Running {test_name} [K={K}, iter={iter_val}, N={N}, dim={dim}]...", end=" ")
    
    # 1. יצירת קלט
    points = generate_input(INPUT_FILE, N, dim)
    
    # ארגומנטים
    args_c = [C_EXEC, str(K)]
    args_py = ["python3", PY_SCRIPT, str(K)]
    if iter_val is not None:
        args_c.append(str(iter_val))
        args_py.append(str(iter_val))
    
    max_iter_internal = iter_val if iter_val else 400

    # 2. הרצת C
    out_c, err_c, code_c = run_program(args_c, INPUT_FILE)
    
    # 3. הרצת Python (שלך)
    out_py, err_py, code_py = run_program(args_py, INPUT_FILE)
    
    # 4. בדיקת שגיאות צפויות (Invalid Input)
    if expect_error:
        success = True
        if out_c != expect_error:
            print(f"\n{RED}[C Failed]{RESET} Expected error '{expect_error}', got: '{out_c}'")
            success = False
        if out_py != expect_error:
            print(f"\n{RED}[Py Failed]{RESET} Expected error '{expect_error}', got: '{out_py}'")
            success = False
        if success: print(f"{GREEN}PASSED (Errors matched){RESET}")
        else: print(f"{RED}FAILED{RESET}")
        return

    # 5. בדיקת ריצה תקינה
    if code_c != 0 or code_py != 0:
        print(f"\n{RED}CRASHED{RESET}")
        if code_c != 0: print(f"C Error: {out_c}")
        if code_py != 0: print(f"Py Error: {out_py}") # לפעמים פייתון מדפיס שגיאות ל-stdout
        return

    # 6. חישוב תוצאת אמת (Internal)
    ref_centroids = run_internal_logic(points, K, max_iter_internal)
    
    # 7. השוואות
    cents_c = parse_centroids(out_c)
    cents_py = parse_centroids(out_py)
    
    c_ok = compare_vectors(ref_centroids, cents_c)
    py_ok = compare_vectors(ref_centroids, cents_py)
    cross_ok = compare_vectors(cents_c, cents_py)
    
    if c_ok and py_ok and cross_ok:
        print(f"{GREEN}PASSED ✅{RESET}")
    else:
        print(f"{RED}FAILED ❌{RESET}")
        if not c_ok: print(f"  - C output differs from Reference.")
        if not py_ok: print(f"  - Python output differs from Reference.")
        if not cross_ok: print(f"  - C and Python outputs differ from each other!")
        
        # הדפסת דוגמה
        print(f"  Ref first: {ref_centroids[0]}")
        if cents_c: print(f"  C   first: {cents_c[0]}")
        if cents_py: print(f"  Py  first: {cents_py[0]}")

def main():
    # קומפילציה ל-C
    print("--- Compiling C Code ---")
    ret = os.system(f"gcc -ansi -Wall -Wextra -Werror -pedantic-errors kmeans.c -o kmeans -lm")
    if ret != 0:
        print(f"{RED}Compilation Failed! Fix C errors first.{RESET}")
        return

    print("\n--- Starting Tests ---")
    
    # טסט 1: רגיל
    run_test("Standard", K=3, iter_val=100, N=50, dim=2)
    
    # טסט 2: ללא iter (ברירת מחדל 400)
    run_test("Default Iter", K=2, iter_val=None, N=40, dim=3)
    
    # טסט 3: ממד גבוה
    run_test("High Dim", K=5, iter_val=50, N=100, dim=10)
    
    # טסט 4: K גדול (אבל תקין)
    run_test("High K", K=10, iter_val=100, N=50, dim=2)

    # --- מקרי קצה ושגיאות ---
    
    # טסט 5: K=1 (לא תקין)
    run_test("Invalid K=1", K=1, iter_val=100, N=50, dim=2, expect_error="Incorrect number of clusters!")
    
    # טסט 6: K >= N (לא תקין)
    run_test("Invalid K>=N", K=20, iter_val=100, N=10, dim=2, expect_error="Incorrect number of clusters!")
    
    # טסט 7: איטרציות לא תקינות (גדול מדי)
    run_test("Invalid Iter > 800", K=3, iter_val=1000, N=50, dim=2, expect_error="Incorrect maximum iteration!")

    # טסט 8: איטרציות לא תקינות (קטן מ-1) - הערה: ב-C שלך זה int, אז 0 לא יעבור, 1.5 ייחתך
    run_test("Invalid Iter <= 1", K=3, iter_val=1, N=50, dim=2, expect_error="Incorrect maximum iteration!")

    # ניקוי
    if os.path.exists(INPUT_FILE):
        os.remove(INPUT_FILE)
    print("\nDone.")

if __name__ == "__main__":
    main()