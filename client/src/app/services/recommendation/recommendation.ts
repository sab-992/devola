import { Injectable, inject } from '@angular/core';
import { environment } from '@environments/environment';
import { HttpService } from '@services/http/http';
import { Observable, map } from 'rxjs';
import { ResumeScore, RecommendationRun, RecommendationResult } from '@models/recommendation';


type RawRecommendationResult = Omit<RecommendationResult, 'scores'> & { scores: string };

@Injectable({ providedIn: 'root' })
export class RecommendationService {
    private readonly m_http = inject(HttpService);

    public fetchResult(uuid: string): Observable<RecommendationResult[]> {
        return this.m_http.get<RawRecommendationResult[]>(`${this.buildPath("/recommendation")}/${uuid}`).pipe(map((results) => results.map((result) => this.parseResult(result))));
    }

    public fetchRuns(): Observable<RecommendationRun[]> {
        return this.m_http.get<RecommendationRun[]>(this.buildPath("/recommendations"));
    }

    public startRecommendation(): Observable<RecommendationRun> {
        return this.m_http.post<RecommendationRun>(this.buildPath("/recommend"));
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