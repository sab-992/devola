import { Injectable, inject } from '@angular/core';
import { environment } from '@environments/environment';
import { HttpService } from '@services/http/http';
import { Observable, ReplaySubject } from 'rxjs';
import { ResumeScore, RecommendationRun, RecommendationResult } from '@models/recommendation';


type RawRecommendationResult = Omit<RecommendationResult, 'scores'> & { scores: string };

@Injectable({ providedIn: 'root' })
export class RecommendationService {
    private readonly m_http = inject(HttpService);

    public fetchResult(uuid: string): Observable<RecommendationResult[]> {
        const sub = new ReplaySubject<RecommendationResult[]>(1);

            this.m_http.get<RawRecommendationResult[]>(`${this.buildPath("/recommendations")}/${uuid}`, {
                next: (results) => {
                    sub.next(results.map((result) => this.parseResult(result)));
                    sub.complete();
                },
                error: (error) => sub.error(error)
            });

        return sub.asObservable();
    }

    public fetchRuns(): Observable<RecommendationRun[]> {
        const sub = new ReplaySubject<RecommendationRun[]>(1);

            this.m_http.get<RecommendationRun[]>(this.buildPath("/recommendations"), { next: (runs) => { sub.next(runs); sub.complete(); },
                                                                                       error: (error) => sub.error(error) });

        return sub.asObservable();
    }

    public startRecommendation(): Observable<RecommendationRun> {
        const sub = new ReplaySubject<RecommendationRun>(1);
        this.m_http.post<RecommendationRun>(this.buildPath("/recommendations"), null, { next: (run) => { sub.next(run); sub.complete(); },
                                                                                        error: (error) => sub.error(error) });
        return sub.asObservable();
    }

    private buildPath(path: string) {
        return `${environment.apiUrl}/rss${path}`;
    }

    private parseResult(raw: RawRecommendationResult): RecommendationResult {
        return { ...raw, scores: this.parseScores(raw.scores) };
    }

    private parseScores(raw: string): ResumeScore[] {
        try {
            const parsed = JSON.parse(raw);
            return Array.isArray(parsed) ? parsed : [];
        } catch {
            return [];
        }
    }
}